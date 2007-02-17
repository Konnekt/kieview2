<div class="row quickevent">
  <div class="context">
    <div class="header">
      {{if showTime?}}<span class="time">{{$time}}</span>{{/if}}
      <span class="type">{{if warning}}warning{{else}}info{{/if}}</span>
    {{unless match?('|<br ?/?>|', body)}}
      <span class="body">{{$body}}</span>
    </div>
      {{else}}
    </div>
    <div class="body">{{$body}}</div>
    {{/unless}}
  </div>
</div>