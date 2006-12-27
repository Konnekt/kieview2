<div class="message unknown">
  <div class="warning"><b>Uwaga !</b> nie rozpoznany typ wiadomoœci !</div>
  <span class="display"><b><TMPL_var htmlEscape(display)></b>:</span>
  <span class="time">(<TMPL_var time>)</span>
  <div class="body">
    <TMPL_if isHtml>
      <TMPL_var body>
    <TMPL_else>
      <pre><TMPL_var htmlEscape(body)></pre>
    </TMPL_if>
  </div>
</div>