// ============================================================================
// C# servlet example for G-WAN Web Application Server(http://trustleap.ch/)
// ----------------------------------------------------------------------------
// loan.cs: Loan monthly amortization calculation in a .NET C# servlet
// ============================================================================
/*    When clients use the loan.html form, we use the form fields to 
      build new content which is then inserted into the original page
     (the javascript code in loan.html will tell you how AJAX works).

      GET and POST forms are processed with the SAME code(the server
      does the URL/Entity parsing for servlets).

      POST is often prefered to GET because the parameters are passed
      in the URL(so they are visible) while POST is using the request
      entity. This difference is *not* relevant in our case because we
      use AJAX to process the query(as a result, the URL is not seen
      by end-users in the Web browser 'address' field).

     This code is released into the public domain.
*/
using System;
using System.Text;

public class loan {
// ----------------------------------------------------------------------------
static string HtmlEncode(string value)
{
    int len = value.Length;
    StringBuilder sb = new StringBuilder(len);
    char c;

    for(int i = 0; i < len; i++)
    {
        c = value [i];
        if(c == ' ')  sb.Append("&#32;");  else
        if(c == '"')  sb.Append("&quot;"); else
        if(c == '&')  sb.Append("&amp;");  else
        if(c == '<')  sb.Append("&lt;");   else
        if(c == '>')  sb.Append("&gt;");   else
        if(c == '\n') sb.Append("&lt;br/&gt;");
        else
        {
           if(c < 32 || c > 127)
           {
              sb.Append("&#");
              sb.Append((int)c);
              sb.Append(';');
           }
           else
              sb.Append(c);
        }
    }
    return sb.ToString();
}
// ----------------------------------------------------------------------------
public static string GetArg(string value) 
{
	return value.Substring(value.IndexOf('=') + 1);
}
// ----------------------------------------------------------------------------
public static int Main(string[] args)
{
   StringBuilder reply = new StringBuilder();
   int nb_args = args.Length != 0 ? args.Length - 1 : 0;

   // ---- no query parameters were provided, redirect client to "loan.html"
   if(nb_args == 0)
   {
      reply.Append("HTTP/1.1 302 Found\r\nContent-type:text/html\r\n"
             + "Location: /csp_loan.html\r\n\r\n"
             + "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
             + "<html><head><title>Redirect</title></head>"
             + "<body>Click <a href=\"/csp_loan.html\">HERE</A> for redirect.</body></html>");
      //lupus commented: Gwan.xbufCat(Gwan.getReply(args[0]), reply.ToString());
      return 302; // HTTP status(302:'Redirect')
   }
   else // ---- if we have query parameters, we process a GET/POST form
   {
      string szName = "";
      string[] Months= new string[] {"January","February","March","April","May","June",
               "July", "August","September","October","November","December"};
      double amount, rate, term, payment, interest, principal, cost;
      int    month = 0, year = 1, lastpayment = 1;

      // the form field "names" we want to find values for 
      string Name = "-", Amount = "0", Rate = "0", Term = "0";
      DateTime start = DateTime.Now;

      // ----------------------------------------------------------------------
      // get the form field values from the URI parameters
      // ----------------------------------------------------------------------
      for(int i = 1; i < args.Length; ++i) 
      {
          switch(args[i][0])
          {
            case 'a': Amount = GetArg(args[i]); break;
            case 'n': Name   = GetArg(args[i]); break;
            case 'r': Rate   = GetArg(args[i]); break;
            case 't': Term   = GetArg(args[i]); break;
         }
      }

      // set default values if a parameter is missing
      if(Name   == null) Name   ="-";
      if(Amount == null) Amount ="100000.0";
      if(Rate   == null) Rate   ="3.5";
      if(Term   == null) Term   ="1.0";

      // ----------------------------------------------------------------------
      // all litteral strings provided by a client must be escaped this way
      // if you inject them into an HTML page
      // ----------------------------------------------------------------------
      szName = HtmlEncode(Name);

      // filter input data to avoid all the useless/nasty cases
      amount = Double.Parse(Amount); 
      if(amount < 1) amount = 1;

      rate = Double.Parse(Rate);   
      if(rate > 19) rate  = 19;  else 
      if(rate >  1) rate /= 100; else 
      if(rate <  1) rate  = 1 / 100;

      term = Double.Parse(Term);   
      if(term < 0.1) term = 1 / 12; else 
      if(term > 800) term = 800;

      // calculate the monthly payment amount
      payment = amount * rate / 12 * Math.Pow(1 + rate / 12, term * 12)
              /(Math.Pow(1 + rate / 12, term * 12) - 1);
      cost =(term * 12 * payment) - amount;

      // build the top of our HTML page
      reply.Append("<!DOCTYPE HTML>"
      + "<html lang=\"en\"><head><title>Loan Calculator</title><meta http-equiv"
      + "=\"Content-Type\" content=\"text/html; charset=utf-8\">"
      + "<link href=\"/imgs/style.css\" rel=\"stylesheet\" type=\"text/css\">"
      + "</head><body style=\"margin:16px;\"><h2>Dear ");
      
      if(szName != "" && szName != "-")
         reply.Append(szName);
      else 
         reply.Append("client");
      
      reply.Append(", your loan goes as follows:</h2>");

      if(term >= 1) 
         term = Convert.ToInt32(term);
      else 
         term = Math.Ceiling(12 * term);

      reply.Append("<br><table class=\"clean\" width=240px>"
            + "<tr><th>loan</th><th>details</th></tr>");
      reply.AppendFormat("<tr class=\"d1\"><td>Amount</td><td>{0:n}</td></tr>", amount);
      reply.AppendFormat("<tr class=\"d0\"><td>Rate</td><td>{0:n}%</td></tr>", rate * 100);
      reply.AppendFormat("<tr class=\"d1\"><td>Term</td><td>{0:n} ", term);
      
      if(term >= 1)
         reply.Append("year");
      else
         reply.Append("month");
               
      reply.AppendFormat("(s)</td></tr><tr class=\"d0\"><td>Cost</td><td>{0:n}", cost);
      reply.AppendFormat("({0:n}%)</td></tr></table>", 100 /(amount / cost));

      reply.AppendFormat("<br><table class=\"clean\" width=112px><tr class=\"d1\"><td><b>YEAR {0:d}</b>", year);
      
      reply.Append("</td></tr></table><table class=\"clean\" width=550px>"
            + "<tr><th>month</th><th>payment</th><th>interest</th>"
            + "<th>principal</th><th>balance</th></tr>");

      for(;;) // output monthly payments
      {
	       month++;
	       interest =(amount * rate) / 12;
	       if(amount > payment)
           {
              amount =(amount - payment) + interest;
              principal = payment - interest;
           }
           else // calculate last payment
           {
              if(lastpayment > 0)
              {
                 lastpayment = 0;
                 payment     = amount;
                 principal   = amount-interest;
                 amount      = 0;
              }
              else // all payments are done, just padd the table
              {
                 amount    = 0;
                 payment   = 0;
                 interest  = 0;
                 principal = 0;
              }
           }

           reply.AppendFormat("<tr class=\"d{0:d}\">", month & 1);
           reply.Append("<td>" + Months[month - 1] + "</td>");
           reply.AppendFormat("<td>{0:n}</td>", payment);
           reply.AppendFormat("<td>{0:n}</td>", interest);
           reply.AppendFormat("<td>{0:n}</td>", principal);
           reply.AppendFormat("<td>{0:n}</td></tr>", amount);

	       if(month == 12)
	       {
              if(amount > 0)
              {
		           month = 0; year++;
                 reply.Append("</table><br><table class=\"clean\" width=112px>"
                             + "<tr class=\"d1\"><td><b>YEAR ");
		          reply.Append(year);
		          reply.Append("</b>"
                      + "</td></tr></table><table class=\"clean\" width=550px>"
                      + "<tr><th>month</th><th>payment</th><th>interest</th>"
                      + "<th>principal</th><th>balance</th></tr>");
              }
              else
                 break;
           }
        }
      
        TimeSpan elapsed = DateTime.Now - start; // not counting code below
      
        // time the process and close the HTML page
        reply.Append("</table><br>This page was generated in ");
        reply.Append(elapsed.TotalMilliseconds);
        reply.Append(" ms.<br>(on a 3GHz CPU 1 ms = "
                    + "3,000,000 cycles)<br></body></html>");
   }

   // -------------------------------------------------------------------------
   // output result
   // -------------------------------------------------------------------------
   //Console.Write(reply.ToString());
   Gwan.xbufCat(Gwan.getReply(args[0]), reply.ToString());
   return 200; // HTTP status(200:'OK')
}}
// ============================================================================
// End of Source Code
// ============================================================================

