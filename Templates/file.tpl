<div class="file">
  Plik
	<TMPL_if name>
	  <span class="filename">"<b><TMPL_var name></b>"</span>
	</TMPL_if>
	<TMPL_if isSent>
		wys³any do
	<TMPL_else>
		odebrany od
	</TMPL_if>
	<span class="display"><b><TMPL_var display></b></span> o
	<span class="time"><TMPL_udf formatTime("%H:%M (%A)", _time)></span>:
	<br/><br/>

	<TMPL_if path>
		<span class="path">Œcie¿ka: 
  	  <TMPL_if path>
  		  <a href="file:///<TMPL_var path>" target="_blank"><TMPL_var path></a> -
  	  </TMPL_if>
  	  <b><a href="file:///<TMPL_var filePath>" target="_blank"><TMPL_var fileName></a></b>
	  </span><br/>
	</TMPL_if>
	<TMPL_if size>
		<span class="size">Rozmiar: <b><TMPL_var size></b></span><br/>
	</TMPL_if>
	<TMPL_if transfered>
		<span class="transfered">
	    <TMPL_if isSent>
	      Przes³ano:
	    <TMPL_else>
	      Odebrano:
	    </TMPL_if>
	    <b><TMPL_var transferedP></b> (<TMPL_var transfered> / <TMPL_var size>)
	  </span><br/>
	</TMPL_if>
	<TMPL_if transferTime>
		<span class="transfer-time">Czas: <b><TMPL_var transferTime></b></span><br/>
	</TMPL_if>
	<TMPL_if avgSpeed>
		<span class="transfer-speed">Œrednia prêdkoœæ: <b><TMPL_var avgSpeed></b></span><br/>
	</TMPL_if>
	<TMPL_if error>
	  <br/>
		<div class="transfer-error">B³¹d: <pre><TMPL_var error></pre></div>
	</TMPL_if>

  <br/>
	<div class="body"><TMPL_var body></div>
</div>