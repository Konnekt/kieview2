<div class="message sms">
  <TMPL_if from>
    <span class="from">od <b><TMPL_var htmlEscape(from)></b></span>
  </TMPL_if>
  <span class="to">do <b><TMPL_var htmlEscape(to)></b></span>
  <span class="time">(<TMPL_var time>)</span>,
  <span class="gate">bramka: <b><TMPL_var gate></b></span>
  <div class="body"><TMPL_var body></div>
</div>