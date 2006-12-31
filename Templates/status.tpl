<div class="status">
  <span class="time">[<b><TMPL_var time></b>]</span>
  <span class="status-desc">
    status <b><TMPL_var status></b>
    <TMPL_if isIgnored>
      - ignoruje Cię
    </TMPL_if>
  </span>
  <TMPL_if info>
    <span class="info">"<TMPL_var htmlEscape(info)>"</span>
  </TMPL_if>
</div>