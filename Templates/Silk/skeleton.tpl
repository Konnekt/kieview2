<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
<html xml:lang="pl" lang="pl" xmlns="http://www.w3.org/1999/xhtml">
<head>
  <base href="{{$tplPath}}/" />
  <!-- 
  <link rel="stylesheet" type="text/css" href="css/default.css" />
  <script type="text/javascript" src="js/prototype.js"></script>
  <script type="text/javascript" src="js/scriptaculous/scriptaculous.js"></script>
  -->
  <script type="text/javascript">
    {{include js/prototype.js}}
  </script>
  <script type="text/javascript">
    function onLoad() {
      try {
        oController = window.external.oController;
        oWindow = window.external.oWindow;

      } catch(e) {
        insertString('<b>Exception</b> (<b>' + e.name + '</b>): ' + e.message);
      }
    }
    window.onload = onLoad;

    function insertString(string) {
      // document.getElementById('container').innerHTML += string;
      new Insertion.Bottom('container', string);
      // $('#container').append(string);
    }
    addMessage = insertString;
    addStatus = insertString;
  </script>
  <link rel="stylesheet" type="text/css" href="css/default.css" />
</head>
<body>
  <div id="container"></div>
</body>
</html>