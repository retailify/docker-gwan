<?php
  /* loan.php: A loan calculator written in PHP 5.4.9.1 (procedural mode)
     compatible with previous  5.2.x and 5.3.x PHP stacks
     to compare with loan.c, loan.cs and loan.java
     Must be launched with 4 parameters such as :
     loan.php&name=Eva&amount=10000&rate=3.5&term=1
  */
  $time_start = microtime(true);
	
  /* convert PHP's command-line arguments into PHP's URI parameters */
  if(PHP_SAPI==="cli") 
  {
     parse_str(implode('&', array_slice($argv, 1)), $_GET);
  }
  /* if we have the 4 query parameters, we process a GET/POST form */
  if ((isset($_GET['name']))&&(isset($_GET['amount']))&&(isset($_GET['rate']))&&(isset($_GET['term']))) 
  {
    $name = $_GET['name']; 
  }
  else 
  { /* we redirect to the /csp_loan.html page */
    ?>
    <!DOCTYPE HTML>
      <html>
        <head>
          <meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
          <title>Loan calculator</title>
          <link type="text/css" rel="stylesheet" href="/www/imgs/style.css"></link>
        </head>
        <body>
          <script language="javascript" type="text/javascript">
            function load_Page(){parent.window.location='/csp_loan.html';}
            window.onload=function(){load_Page();}
          </script>
        </body>
      </html>
    <?php
  }

  if($name == '' || $name == '-' || $name == ' ') 
  {
    $name = "client";
  }

  if($name != '') 
  {
    $monthList = array("January", 
                       "February", 
                       "March", 
                       "April", 
                       "May", 
                       "June", 
                       "July", 
                       "August", 
                       "September", 
                       "October",
                       "November", 
                       "December", 
                       "");

    if (isset($_GET['amount'])) 
    {
      $amount = floatval(trim($_GET['amount'])); 
    }
       
    if ($amount == '' || $amount == 0 || $amount == ' ') 
    {
      $amount = 100000.00;
    }

    if($amount < 1) 
    {
      $amount = 100000.00;
    }

    if (isset($_GET['rate'])) 
    {
      $rate = floatval(trim($_GET['rate']));
    }

    if ($rate == '' || $rate == 0 || $rate == ' ') 
    {
      $rate = 3.5;
    }
    else 
    {
      $rate = abs($rate);
    }

    if($rate > 20) /* to limit the damage - LoL */
    { 
      $rate = 20;
    }

    if($rate > 1) 
    {
      $rate = abs($rate / 100.);
    }
    else 
    {
      $rate = 3.5 / 100.;
    }

    if (isset($_GET['term'])) 
    {
      $term = floatval(trim($_GET['term']));
    }

    if ($term == '' || $term < 1 || $term == ' ') 
    {
      $term = 1;
    }
    elseif ($term > 800) /* to limit the damage - LoL */
    {
      $term = 800;
    }
    else 
    {
      $term = round(abs($term));
    }

    $name = addslashes($name);
    $year = 1; 
    $lastpayment = 1;
    /* all literal strings provided by a client must be escaped this way
     * if you inject them into an HTML page
     */
    htmlspecialchars($name);
    /* calculate the montly payment amount */
    $payment = $amount*$rate/12*pow(1+$rate/12,$term*12)/(pow(1+$rate/12,$term*12)-1);
    /* build the top of our HTML page */
    $cost = ($term*12*$payment)-$amount;
    $result  = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
    $result .= sprintf("<html>" .
                       "<head>" .
                       "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />" .
                       "<title>Loan calculator</title>" .
                       "<link type=\"text/css\" rel=\"stylesheet\" href=\"imgs/style.css\"></link>" .
                       "</head>" .
                       "<body>" .
                       "<h2>Dear $name, your loan goes as follows:</h2>");
    $result .= sprintf("<br/><table class=\"clean\" width=240px>" .
                       "<tr><th>loan</th><th>details</th></tr>" .
                       "<tr class=\"d1\"><td>Amount</td><td>%s</td></tr>" .
                       "<tr class=\"d0\"><td>Rate</td><td>%.2f%%</td></tr>" .
                       "<tr class=\"d1\"><td>Term</td><td>%u %s(s)</td></tr>" .
                       "<tr class=\"d0\"><td>Cost</td><td>%s (%.2f%%)</td></tr>" .
                       "</table>", number_format($amount, 2), $rate*100, 
                       $term,($term>0)?"year":"month", 
                       number_format($cost,2), 100/($amount/$cost));
                       
    $result .= sprintf("<br/><table class=\"clean\" width=112px>" .
                       "<tr class=\"d1\"><td><b>YEAR %u</b></td></tr></table>" .
                       "<table class=\"clean\" width=550px>" .
                       "<tr><th>month</th><th>payment</th><th>interest</th>" .
                       "<th>principal</th><th>balance</th></tr>", $year);
                       
    for($month = 1; $month <= 12; $month++) // ouput monthly payments
    { 
      $interest = ($amount*$rate)/12;
      if($amount > $payment) 
      {
        $amount = ($amount-$payment) + $interest; 
        $principal = $payment - $interest;
      }
      else // calculate last payment
      { 
        if($lastpayment > 0) 
        {
          $lastpayment = 0;
          $payment = $amount;
          $principal = $amount - $interest;
          $amount = 0;
        }
        else // all payments are done, just pad the table
        { 
          $amount = 0;
          $payment = 0;
          $interest = 0;
          $principal = 0;
        }                
      }
      $result .= sprintf("<tr class=\"d%u\"><td>%s</td><td>%s</td><td>" .
                         "%s</td><td>%s</td><td>%s</td></tr>",
                         $month%2, $monthList[$month-1], 
                         number_format($payment, 2), 
                         number_format($interest, 2), 
                         number_format($principal, 2), 
                         number_format($amount, 2));
      if($month == 12) 
      {
        if($amount) 
        {
          $month = 0; 
          $year++;
          $result .= sprintf("</table><br><table class=\"clean\" " .
                             "width=112px><tr class=\"d1\"><td><b>YEAR %u</b>" .
                             "</td></tr></table><table class=\"clean\" width=550px>" .
                             "<tr><th>month</th><th>payment</th><th>interest</th>" .
                             "<th>principal</th><th>balance</th></tr>",
                             $year);
        }
      }
    }
    /* time the process, print the HTML page and close the program with a "200" return code */
    $result .= sprintf("</table><br>This page was generated in %.2F ms.</br>(on a 3GHz CPU 1 ms = 3,000,000 cycles)".
    "</br></body></html>",number_format(((microtime(true)-$time_start)*1000),2));
    echo $result;
    exit(200);
  }
?>

