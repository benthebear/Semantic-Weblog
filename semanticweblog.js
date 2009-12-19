// @author Benjamin Birkenhake <benjaminbirkenhake.org>
// @package semanticweblog
// @copyright creative commons by-sa 


function semanticweblog_send_assoc(){
  // Create all necessary Variables
  var term1 = '';
  var atid = '';
  var term2 = '';
  var target = 'result';
  var url = 'http://anmutunddemut.de/association/add';
  var method = 'GET';
  var append = true;
  term1 = document.getElementById('edit-role1-ajax').value;
  atid = document.getElementById('edit-atid-ajax').value;
  term2 = document.getElementById('edit-role2-ajax').value;
  url += '/' + term1 + '/' + atid + '/' + term2;
  //alert (url);
  
  if (document.getElementById) { // Only "document.getElementById" Browsers are allowed beyond here
     // Browsergate: IE uses ActiveX, all other can go
     var http_request = (window.ActiveXObject) ? new ActiveXObject("Microsoft.XMLHTTP") : new XMLHttpRequest();
  }//if

  http_request.onreadystatechange =
  function() {
     if ((http_request.readyState == 4) && (http_request.status == 200)) {
        if(append) {
           document.getElementById(target).innerHTML += "<br/>"+http_request.responseText; // Attach the Result
        }else{
           document.getElementById(target).innerHTML = http_request.responseText; // Or Replace the Result
        }
        document.getElementById(target).scrollTop = document.getElementById(target).scrollHeight;       
     }// if
  }//function
  ;
  http_request.open(method,url,true); // Get the data, which file?, loading asynchronously is true
  http_request.send(null); // Es werden keine Daten transferiert, darum
  // The two Term fields are being cleared
  document.getElementById('edit-role1-ajax').value = '';
  document.getElementById('edit-role2-ajax').value = '';
}//function
