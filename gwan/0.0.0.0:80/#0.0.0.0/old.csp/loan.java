// ============================================================================
// Java servlet example for the G-WAN Web App. Server (http://trustleap.ch/)
// ----------------------------------------------------------------------------
// loan.java: G-WAN's version of the Java loan http://gwan.ch/source/loan.jsp
//            written for GlassFish (follow the link above for details).
// ----------------------------------------------------------------------------
// This Java code is served very quickly by G-WAN (despite C/Java conversions). 
// See below (the " -t 6" option setups 6 client worker threads for 6 Cores):
// 
// weighttp -n 1000000 -c 300 -t 6 -k -H "Accept-Encoding: gzip" \
// "http://127.0.0.1:80/?loan.java&name=Eva&amount=10000&rate=3.5&term=1"
// finished in 1 sec, 748 millisec, 571778 req/s, 1286500 kbyte/s
// requests: 1000000 total/started/done/succeeded, 0 failed/errored
// status codes: 1000000 2xx, 0 3xx, 0 4xx, 0 5xx
// traffic: 2303999915 bytes total, 276999915 bytes http, 2027000000 bytes data
//
// On the same machine, Tomcat's HelloWorld example does:
// (installation: https://help.ubuntu.com/11.04/serverguide/C/tomcat.html)
// (administration https://help.ubuntu.com/community/ApacheTomcat5)
// 
// weighttp -n 1000000 -c 300 -t 6 -k -H "Accept-Encoding: gzip" \
// "http://127.0.0.1:8080/examples/servlets/servlet/HelloWorldExample"
// finished in 12 sec, 278 millisec, 81441 req/s, 39024 kbyte/s
// requests: 1000000 total/started/done/succeeded, 0 failed/errored
// status codes: 1000000 2xx, 0 3xx, 0 4xx, 0 5xx
// traffic: 490671704 bytes total, 131671704 bytes http, 359000000 bytes data
// 
//   Server    bytes sent    kbyte/sec  requests/sec   servlet     time
//   ------  --------------  ---------  ------------  ----------  ------
//   G-WAN    2,303,999,915  1,286,500       571,778   loan.java   1 sec
//   Tomcat     490,671,704     39,024        81,441  hello.java  12 sec
// 
// The next best Web servers do not even reach 571,778 req/s when serving mere
// static contents. G-WAN is making dynamic contents fly (high).
// ----------------------------------------------------------------------------
import api.Gwan;

public class loan // the class name MUST match the script *.java filename
{
   public static int jmain(long env, String[] args) 
   {
      String m_out = "";

      // ---- requests without parameters trigger a redirect to the input form
      if(args.length == 0)
      {
         m_out = "HTTP/1.1 302 Found\r\n"
            + "Content-type:text/html\r\n"
            + "Location: /csp_loan.html\r\n\r\n"
            + "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
            + "<html><head><title>Redirect</title></head>"
            + "<body>Click <a href=\"/csp_loan.html\">HERE</A> for redirect.</body>"
            + "</html>";
      }
      else
      {
         // ---- process a GET query with parameters
         long start = Gwan.getUs();
	      double payment, interest, principal, cost, amount, rate, term;
	      int    month=0, year=1, lastpayment=1;
         String[] Months={"January","February","March","April","May","June",
                  "July", "August","September","October","November","December"};

         // the form field "names" we want to find values for
         String szName="-", Name="", Amount="0.0", Rate="0.0", Term="0.0";

         // anyone volunter to write a Java equivalent for G-WAN's get_arg()?
         switch(args[0].charAt(0))
         {
             case 'a': Amount = args[0].substring(args[0].indexOf('=') + 1); break;
             case 'n': Name   = args[0].substring(args[0].indexOf('=') + 1); break;
             case 'r': Rate   = args[0].substring(args[0].indexOf('=') + 1); break;
             case 't': Term   = args[0].substring(args[0].indexOf('=') + 1); break;
         }
         switch(args[1].charAt(0))
         {
             case 'a': Amount = args[1].substring(args[1].indexOf('=') + 1); break;
             case 'n': Name   = args[1].substring(args[1].indexOf('=') + 1); break;
             case 'r': Rate   = args[1].substring(args[1].indexOf('=') + 1); break;
             case 't': Term   = args[1].substring(args[1].indexOf('=') + 1); break;
         }
         switch(args[2].charAt(0))
         {
             case 'a': Amount = args[2].substring(args[2].indexOf('=') + 1); break;
             case 'n': Name   = args[2].substring(args[2].indexOf('=') + 1); break;
             case 'r': Rate   = args[2].substring(args[2].indexOf('=') + 1); break;
             case 't': Term   = args[2].substring(args[2].indexOf('=') + 1); break;
         }
         switch(args[3].charAt(0))
         {
             case 'a': Amount = args[3].substring(args[3].indexOf('=') + 1); break;
             case 'n': Name   = args[3].substring(args[3].indexOf('=') + 1); break;
             case 'r': Rate   = args[3].substring(args[3].indexOf('=') + 1); break;
             case 't': Term   = args[3].substring(args[3].indexOf('=') + 1); break;
         }

         // set default values if a parameter is missing
         if(Name   == null) Name   ="-";
         if(Amount == null) Amount ="100000.0";
         if(Rate   == null) Rate   ="3.5";
         if(Term   == null) Term   ="10.0";

         // all litteral strings provided by a client must be escaped this way
         // if you inject them into an HTML page
         //public static String escape_html(String Name)
         {
             int len = Name.length();
             StringBuffer sb = new StringBuffer(len);
             int c;

             for(int i = 0; i < len; i++)
             {
                 c = Name.charAt(i);
                 if(c == ' ')  sb.append("&#32;");  else
                 if(c == '"')  sb.append("&quot;"); else
                 if(c == '&')  sb.append("&amp;");  else
                 if(c == '<')  sb.append("&lt;");   else
                 if(c == '>')  sb.append("&gt;");   else
                 if(c == '\n') sb.append("&lt;br/&gt;");
                 else
                 {
                    c = c&0xffff; // unicode
                    if(c < 32 || c > 127)
                    {
                       sb.append("&#");
                       sb.append(new Integer(c).toString());
                       sb.append(';');
                    }
                    else
                       sb.append(c);
                 }
             }
             //return sb.toString();
             szName = sb.toString();
         }

         // filter input data to avoid all the useless or nasty cases
         amount = Double.parseDouble(Amount);
         if(amount < 1.0)
            amount = 1.0;

         rate = Double.parseDouble(Rate);
         if(rate > 19.0)
            rate = 20.0;

         if(rate > 1.0)
            rate = rate/100.0;
         else
         if(rate < 1.0)
            rate  = 1.0/100.0;

         term = Double.parseDouble(Term);
	      if(term < 0.1)
	         term = 1.0/12.0;
         // do not do this in production...
	      //else
	      //if(term > 90.0)
         //   term = 90.0;

         // calculate the monthly payment amount
         payment = amount*rate/12*Math.pow(1+rate/12, term*12)
                 /               (Math.pow(1+rate/12, term*12)-1);
         cost    = (term*12*payment)-amount;

         // build the top of our HTML page
         m_out += String.format(
           "<!DOCTYPE HTML>"
         + "<html lang=\"en\"><head><title>Loan Calculator</title><meta http-equiv"
         + "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
         + "<link href=\"/imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
         + "</head><body style=\"margin:16px;\">"
         + "<h2>Dear %s, your loan goes as follows:</h2><br>", 
         (Name.length() < 2 || Name.charAt(0) == '-') ? "client" : Name);
         // the HTML escaping needs a fix, any volunteer?
         //(szName.length() < 2 || szName.charAt(0) == '-') ? "client" : szName);

         m_out += String.format(
                    "<table class=\"clean\" width=240px>"
                   +"<tr><th>loan</th><th>details</th></tr>"
                   +"<tr class=\"d1\"><td>Amount</td><td>%,.2f"
                   +"</td></tr><tr class=\"d0\"><td>Rate</td><td>"
                   +"%,.2f%%</td></tr><tr class=\"d1\"><td>Term</td><td>"
                   +"%d %s(s)</td></tr><tr class=\"d0\"><td>Cost</td><td>"
                   +"%,.2f (%3.2f%%)</td></tr></table>",
                    amount,
                    rate * 100,
                    (int)(((int)term>0)?term:Math.ceil(12*term)),
                    ((int)term>0)?"year":"month",
                    cost,
                    100/(amount/cost));

	      m_out += String.format(
	                 "<br><table class=\"clean\" width=112px>"
                   +"<tr class=\"d1\"><td><b>YEAR %d"
                   +"</b></td></tr></table>"
                   +"<table class=\"clean\" width=550px>"
                   +"<tr><th>month</th><th>payment</th><th>interest</th>"
                   +"<th>principal</th><th>balance</th></tr>",
                    year);

         // output monthly payments
	      for(;;)
	      {
		      month    = month+1;
      	       interest = (amount*rate)/12;
		      if(amount>payment)
            {
               amount    = (amount-payment)+interest;
		         principal = payment-interest;
            }
            else
            {
               // calculate last payment
               if(lastpayment>0)
               {
                  lastpayment = 0;
                  payment     = amount;
                  principal   = amount-interest;
                  amount      = 0;
               }
               else
               {
                  // all payments are done, just padd the table
                  amount    = 0;
                  payment   = 0;
                  interest  = 0;
                  principal = 0;
               }
            }

            m_out += String.format(
                         "<tr class=\"d%d\"><td>%s</td><td>%,.2f</td><td>%,.2f"
                        +"</td><td>%,.2f</td><td>%,.2f</td></tr>",
                         month & 1, Months[month - 1],
                         payment,
                         interest,
                         principal,
                         amount);

		      if(month==12)
		      {
               if(amount>0)
               {
			         month=0; year=year+1;
	               m_out += String.format(
	                            "</table><br><table class=\"clean\" width=112px>"
                              +"<tr class=\"d1\"><td><b>YEAR %d"
                              +"</b></td></tr></table>"
                              +"<table class=\"clean\" width=550px>"
                              +"<tr><th>month</th><th>payment</th><th>interest"
                              +"</th><th>principal</th><th>balance</th></tr>",
                               year);
               }
               else
               {
                  break;
               }
            }

         }

         // time the process and close the HTML page
         m_out += String.format(
                    "</table><br>This page was generated in %,.2f ms."
                   +"<br>(on a 3GHz CPU 1 ms = 3,000,000 cycles)"
                   +"<br></body></html>", (Gwan.getUs() - start)/1000.0);

      }

      Gwan.xbufCat(Gwan.getReply(env), m_out);
      return 200;
   }
}
// ============================================================================
// End of Source Code
// ============================================================================

