// ============================================================================
// loan.c: the command-line equivalent of the G-WAN loan.c servlet.
//
//         Just to compare the time taken by a stand-alone loan.c to generate 
//         the loan HTML page to the time taken by G-WAN's loan.c servlet:
//
//         G-WAN loan.c ....... 0.40 ms (G-WAN is 15x faster)
//         GCC   loan.c ....... 5.67 ms
//
//         Since G-WAN relies on GCC to compile C servlets like loan.c, this
//         15x difference comes from the G-WAN C API (the only difference).
//
//         Here, there is no concurrency since the loan.c code is executed one
//         time only.
//
//         Note that in this test, the GCC version is advantaged as it does not
//         escape HTML, does not parse URI parameters, it calls local functions
//         instead of the foreign G-WAN API, and the execuatale size if much 
//         smaller (17.2 KiB for the standalone loan executable).
//
//         Compile with: gcc loan.c -O3 -o loan -lrt
//         Test machine: 6-Core Xeon CPU W3680 @ 3.33GHz, Ubuntu 12.04.1 LTS
// ============================================================================
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef   signed char       s8;
typedef unsigned char       u8;
typedef   signed short int s16;
typedef unsigned short int u16;
typedef   signed int       s32;
typedef unsigned int       u32;

typedef unsigned long      ulong;

# ifdef _WIN32
typedef          __int64   s64;
typedef unsigned __int64   u64;
# else
#  if __WORDSIZE == 64
typedef          long int s64;
typedef long unsigned int u64;
#  else
typedef          long long int s64;
typedef long long unsigned int u64;
#  endif
# endif

// from gwan.h
#define HTTP_200_OK                            200
#define HTTP_302_MOVED_TEMPORARILY             302

// ----------------------------------------------------------------------------
static u64 getus(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   return (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
}
// ----------------------------------------------------------------------------
// like atof() -but here you can *filter* input strings ("%16.2f" here)
// ----------------------------------------------------------------------------
static long double atod(char *p)
{
	u32 sign = 0;
   while(*p == ' ') p++; // pass space characters
   switch(*p)
   {
      case '-': sign=1;
      case '+': p++;
   }
   
   long double d = 0.0, factor = 0.1;
   u32 v, i = 16; // i:the maximum integral part we want to scan
   while(i && (v=*p) && v >= '0' && v <= '9') // integer part
      p++, d = 10. * d + (v - '0'), i--;
	
   if(*p == '.' || *p == ',') // fractional part
   {
		p++; i = 2; // i:the maximum fractional precision we want to scan
		while(i && (v=*p) && v >= '0' && v <= '9')
		   p++, d += (v - '0') * factor, factor *= 0.1, i--;
	}
	return sign?-d:d;   
}
// ----------------------------------------------------------------------------
// return the smallest integer greater than 'x'
// ----------------------------------------------------------------------------
static inline u32 uceil(double x)
{
   u32 ipart = (int)x;
   if(x - ipart) return(x < 0.0) ? ipart : ipart + 1;
   else          return(ipart);
}
// ----------------------------------------------------------------------------
// raise 'x' to the power of 'p'
// ----------------------------------------------------------------------------
static inline long double powd(long double x, u32 p)
{
   switch(p)
   {
      case 0: return 1;
      case 1: return x;
   }

   long double res = 1;
   do
   {  // for each bit of power 'p', multiply 'res' by the x^bit factor
      if(p & 1)    //                               [8421]
         res *= x; // x^10 = x^8 * x^2 (power=10 is "1010")

      x *= x, p >>= 1; // get next 'p' bit
   } 
   while(p); // if(p==1024) 10 iterations, instead of 1024: while(p--) x *= x;
   return res;
}
// ----------------------------------------------------------------------------
static char *get_arg(char *search, char **val, int argc, char *argv[])
{
   int i = argc;
   while(i--)
   {
      int len = strlen(search);
      if(strstr(argv[i], search))
      {
         *val = argv[i] + len;
         return argv[i] + len;
      }
   }
   return NULL;
}
// ----------------------------------------------------------------------------
// main() is receiving the query parameters ("csp?arg1&arg2&arg3...") in argv[]
// ----------------------------------------------------------------------------
int main(void)
{
   int argc = 4; // "/?loan.c&name=Eva%..." URI parameters are already parsed
   char *argv[] = { "name=Eva", "amount=100000", "rate=3.5", "term=100", 0 };

   // -------------------------------------------------------------------------
   // no query parameters were provided, redirect client to "loan.html"
   // -------------------------------------------------------------------------
   if(argc < 2)
   {
      static char redir[] = "HTTP/1.1 302 Found\r\n"
         "Content-type:text/html\r\n"
         "Location: csp_loan.html\r\n\r\n"
         "<html><head><title>Redirect</title></head><body>"
         "Click <a href=\"csp_loan.html\">here</a>.</body></html>";

      puts(redir);
      return HTTP_302_MOVED_TEMPORARILY; // return an HTTP code
   }
   
   setlocale(LC_NUMERIC, "en_US.iso88591"); // get a thousands separator

   char *ptr; size_t reply_size = 0;
   FILE *reply = open_memstream(&ptr, &reply_size);   
   
   // -------------------------------------------------------------------------
   // if we have query parameters, we process a GET/POST form (the same way)
   // -------------------------------------------------------------------------
   char *Months[] = {"January", "February", "March", "April", "May",
                     "June", "July", "August", "September", "October",
                     "November", "December"};
   char szName[400] = {0};
   double amount, down, rate, term, payment, interest, principal, cost;
   int month = 0, year = 1, lastpayment = 1;

   // the form field "names" we want to find values for 
   char *Name = "", *Amount = "0", *Down = "0", *Rate = "0", *Term = "0";
   //u64 start = cycles64(); // current CPU clock cycles' counter
   u64 start = getus(); // elapsed micro-seconds (1 us = 1,000 milliseconds)

   // get the form field values (note the ending '=' name delimiter)
   get_arg("name=",   &Name,   argc, argv);
   get_arg("amount=", &Amount, argc, argv);
   get_arg("rate=",   &Rate,   argc, argv);
   get_arg("down=",   &Down,   argc, argv); // amount/percentage paid upfront
   get_arg("term=",   &Term,   argc, argv);
   
   printf("Name:%s  Amount:%s  Rate:%s  Term:%s\n", Name, Amount, Rate, Term);

   // all litteral strings provided by a client must be escaped this way
   // if you inject them into an HTML page
   //escape_html((u8*)szName, (u8*)Name, sizeof(szName) - 1);
   strcpy(szName, Name);
   
   // but we don't want to display "%20" for each space character
   {
      char *s = szName, *d = s;
      while(*s)
      {
         if(s[0] == '%' && s[1] == '2' && s[2] == '0') // escaped space?
         {
            s += 3;     // pass escaped space
            *d++ = ' '; // translate it into the real thing
            continue;   // loop
         }
         
         *d++ = *s++; // copy other characters
      }
      *d = 0; // close zero-terminated string
   }

   // filter input data to avoid all the useless/nasty cases
   amount = atod(Amount);
   if(amount < 1)
      amount = 1;

   down = atod(Down);
   if(down > 0)
   {
      if(down < 100) // convert the percentage into an amount
         down = amount * (down / 100.);

      amount -= down;
   }
   
   rate = atod(Rate);
   if(rate > 1)
   {
      if(rate > 19) // limit the damage...
         rate = 20;
         
      rate /= 100;
   }
   else
   if(rate > 0)
      rate = 1. / 100.;
   else
      rate = .000001;
   
   term = atod(Term);
   if(term < 0.1)
      term = 1. / 12.;
   else 
   if(term > 800) term = 800;

   // calculate the monthly payment amount
   payment = amount * (rate/12 / (1 - powd(1 / (1 + rate/12), term*12)));
   cost = (term * 12 * payment) - amount;

   // build the top of our HTML page
   fprintf(reply, "<!DOCTYPE HTML>"
      "<html lang=\"en\"><head><title>Loan Calculator</title><meta http-equiv"
      "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
      "<link href=\"/imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
      "</head><body style=\"margin:16px;\">"
      "<h2>Dear %s, your loan goes as follows:</h2><br>", 
      (*szName && *szName != '-') ? szName : "client");  

   fprintf(reply, "<table class=clean width=240px>"
      "<tr><th>loan</th><th>details</th></tr>"
      "<tr class=d1><td>Amount</td><td>%'.2F</td></tr>"
      "<tr class=d0><td>Rate</td><td>%'.2F%%</td></tr>"
      "<tr class=d1><td>Term</td><td>%u %s(s)</td></tr>"
      "<tr class=d0><td>Cost</td><td>%'.2F (%'.2F%%)</td></tr>"
      "</table>", amount, rate * 100,
            ((u32)term)?((u32)term):uceil(12 * term),
            ((u32)term)?"year":"month", cost, 100 / (amount / cost));

   fprintf(reply, "<br><table class=clean width=112px>"
      "<tr class=d1><td><b>YEAR %u</b></td></tr></table>"
      "<table class=clean width=550px>"
      "<tr><th>month</th><th>payment</th><th>interest</th>"
      "<th>principal</th><th>balance</th></tr>", year);

   for(;;) // output monthly payments
   {
      month++;
      interest = (amount * rate) / 12;

      if(amount > payment)
      {
         principal = payment - interest;
         amount -= principal;
      }
      else // calculate last payment
      {
         if(lastpayment)
         {
            lastpayment = 0;
            payment = amount;
            principal = amount - interest;
            amount = 0;
         }
         else // all payments are done, just padd the table
         {
            amount = 0;
            payment = 0;
            interest = 0;
            principal = 0;
         }
      }

      fprintf(reply, "<tr class=d%u><td>%s</td><td>%'.2F</td><td>%'.2f</td>"
             "<td>%'.2f</td><td>%'.2f</td></tr>", 
               month & 1, Months[month - 1], 
               payment, interest, principal, amount);

      if(month == 12)
      {
         if(amount)
         {
            month = 0;
            year++;
            fprintf(reply, "</table><br><table class=clean width=112px>"
                     "<tr class=d1><td><b>YEAR %u</b></td></tr></table>"
                     "<table class=clean width=550px>"
                     "<tr><th>month</th><th>payment</th><th>interest</th>"
                     "<th>principal</th><th>balance</th></tr>", year);
         }
         else
            break;
      }
   }

   // -------------------------------------------------------------------------
   // time the process and close the HTML page
   /* -------------------------------------------------------------------------
   fprintf(reply, 
            "</table><br>This page was generated in %llU CPU clock cycles."
            "<br>(on a 3GHz CPU 1 ms = 3,000,000 cycles)"
            "<br></body></html>", cycles64() - start);*/
   fprintf(reply, 
            "</table><br>This page was generated in %'.2F ms."
            "<br>(on a 3GHz CPU 1 ms = 3,000,000 cycles)"
            "<br></body></html>", (getus() - start)/1000.0);
            
   fclose(reply); // 'ptr' now points to the 'reply' buffer
   puts(ptr);     // let's see the HTML

   return HTTP_200_OK; // return an HTTP code
}
// ============================================================================
// End of Source Code
// ============================================================================
