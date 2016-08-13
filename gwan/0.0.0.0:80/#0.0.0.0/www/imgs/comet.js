// ----------------------------------------------------------------------------
// ajax.js: AJAX Requests and Streaming
// ----------------------------------------------------------------------------
function getRequest()
{
   try{return new XMLHttpRequest();}catch(e){}
   try{return new ActiveXObject("Msxml2.XMLHTTP.6.0");}catch(e){}
   try{return new ActiveXObject("Msxml2.XMLHTTP.3.0");}catch(e){}
   try{return new ActiveXObject("Msxml2.XMLHttp");}catch(e){}
   try{return new ActiveXObject("Microsoft.XMLHTTP");}catch(e){}
   return null;
}
// ----------------------------------------------------------------------------
function ajaxGet(url, method, readFn, failFn)
{
   request = getRequest();
   request.open(method, url, true);
   request.setRequestHeader("Content-type","application/x-www-form-urlencoded;");
   //request.setRequestHeader("Connection","close");
   var lastSize;
   request.onreadystatechange = function()
   { 
      if(typeof request == "undefined") return 0;
      
      // 0:UNSENT 
      //   The object has been constructed.
      //
      // 1:OPENED 
      //   The open() method has been successfully invoked. 
      //   During this state request headers can be set using 
      //   setRequestHeader() and the request can be made using
      //   the send() method.
      //
      // 2:HEADERS_RECEIVED 
      //   All redirects (if any) have been followed and all 
      //   HTTP headers of the final response have been received.
      //   Several response members of the object are now available.
      //
      // 3:LOADING
      //   The response entity body is being received.
      //
      var newTextReceived;
      if(request.readyState > 2) // get the newest text
      {
         newTextReceived = request.responseText.substring(lastSize);
         lastSize = request.responseText.length;
         readFn(newTextReceived);
      }

      // 4:DONE
      //   The data transfer has been completed or something 
      //   went wrong during the transfer (e.g. infinite redirects).
      //
      if(request.readyState == 4) // 4: request finished and response is ready
      {
        if(request.status == 200)
        {
           delete request;
           ajaxGet(url, method, readFn, failFn); // response done: create new request
        }   
        else 
        if(failFn != null)
        {
           failFn(request);
        }  
        return request.status;
      }
      return 0;
   }
   // argument: string for POST requests not using x-www-form-urlencoded
   request.send(null); 
}
// ----------------------------------------------------------------------------
// End of Source Code
// ----------------------------------------------------------------------------

