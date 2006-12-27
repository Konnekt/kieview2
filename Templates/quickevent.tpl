<div class="message quickevent">
  <TMPL_if showTime>
    <span class="time">[<b><TMPL_var time></b>]</span>
  </TMPL_if>
  <div class="body">
    <TMPL_if isHtml>
      <TMPL_var body>
    <TMPL_else>
      <pre><TMPL_var htmlEscape(body)></pre>
    </TMPL_if>
    <TMPL_if warning>
      ! <b>warning</b> !
    </TMPL_if>
  </div>
</div>