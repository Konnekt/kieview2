{{if grouped}}
<div class="row grouped status" style="margin-top: -10px">
  <div class="context-grouped">
    <span class="floating time">{{if groupTime}}<b>{{$timeFromLastSt}}</b> p�niej{{else}}{{$time}}{{/if}}</span>
    {{unless groupStatus}}<span class="floating name">{{$status}}</span>{{/unless}}
    <div class="body">{{if info}}{{$info}}{{else}}<em>void</em>{{/if}}</div>
  </div>
</div>
{{else}}
<div class="row status">
  <div class="context">
    <span class="closer" onclick="$(this.parentNode.parentNode).hideThread('slow');">x</span>
    <div class="header">
      <span class="name">{{$status}}</span>
      <span class="time">{{$time}}</span>
    </div>
    {{if info}}<div class="body">{{$info}}</div>{{/if}}
  </div>
</div>
{{/if}}