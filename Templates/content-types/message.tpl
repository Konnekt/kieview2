<div class="message">
  <span class="display"><b><TMPL_var htmlEscape(display)></b>:</span>
  <span class="time">(<TMPL_var time>)</span>
  <div class="body">
    <TMPL_if html?>
      <TMPL_var body>
    <TMPL_else>
      <pre><TMPL_var htmlEscape(body)></pre>
    </TMPL_if>
  </div>
</div>