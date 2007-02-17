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

        //Coontroller.ieVersion = function() { return 9 + 1; };
        //oController.ieVersion = 9;
        //oController.justSS = function() { return "blaa"; };
        //alert($H(oController).inspect());
        //oWindow.minimize();
        //insertString(oController.getPluginVersion('xkAway2'));
        //alert(oController.getEmot(4716));
        
        /*
        x.wazaa = 'tak';
        insertString(x.wazaa() + ' | ');
        insertString(x.inspect() + '<br/>');
        insertString(typeof x.xxx);
        insertString(x.zzz.inspect());
        if (typeof(someFuckinVar) != 'undefined') insertString(9);
        insertString(x.yyy(10, 3));
        insertString(someFuckinVar);
        insertString(x[6].test('xxxorg'));
        insertString('xxx2' in x);
        insertString('<br/><br/>' + x.ieVer);
        */

      } catch(e) {
        alert($H(e).inspect());
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