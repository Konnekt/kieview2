<div class="status">
  <span class="time">[<b>{{$time}}</b>]</span>
  <span class="status-desc">
    status <b>{{$status}}</b> {{if ignored?}}- ignoruje Ci�{{/if}}
  </span>
  {{if info}}
    <span class="info">"{{$info}}"</span>
  {{/if}}
</div>