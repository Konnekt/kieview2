<div class="message quickevent">
  <TMPL_if showTime?>
    <span class="time">[<b><TMPL_var time></b>]</span>
  </TMPL_if>
  <div class="body">
    <TMPL_if warning?>! <b>warning</b> !</TMPL_if>
    <TMPL_var body>
  </div>
</div>