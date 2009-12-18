function semanticweblog_send_assoc(){
  // Create all necessary Variables
  var term1 = '';
  var atid = '';
  var term2 = '';
  var target = 'result';
  var url = 'http://anmutunddemut.de/association/add';
  var method = 'GET';
  var append = true;
  term1 = document.getElementById('edit-role1-1').value;
  atid = document.getElementById('edit-atid-1').value;
  term2 = document.getElementById('edit-role2-1').value;
  url += '/' + term1 + '/' + atid + '/' + term2;
  //alert (url);
  
  if (document.getElementById) { // Nur Browser die "document.getElementById" koennen duerfen weitermachen
     // Browserweiche: IE braucht ActiveX, alle anderen koennen es direkt (if else Geschichte)
     var http_request = (window.ActiveXObject) ? new ActiveXObject("Microsoft.XMLHTTP") : new XMLHttpRequest();
  }//if

  http_request.onreadystatechange =
  function() {
     if ((http_request.readyState == 4) && (http_request.status == 200)) {
        if(append) {
           document.getElementById(target).innerHTML += "<br/>"+http_request.responseText; // Dranhängen
        }else{
           document.getElementById(target).innerHTML = http_request.responseText; // Überschreiben
        }
        document.getElementById(target).scrollTop = document.getElementById(target).scrollHeight;
       
        //document.getElementById("loading").style.display = "none";
     }// if
  }//function
  ;
  http_request.open(method,url,true); // Get the data, which file?, loading asynchronously is true
  http_request.send(null); // Es werden keine Daten transferiert, darum
}//function
