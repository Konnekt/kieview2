{{if grouped}}
<div class="row grouped {{if sent?}}outgoing{{else}}incoming{{/if}}" style="margin-top: -10px">
  <div class="context-grouped">
    <span class="floating time">{{if groupTime}}<b>{{$timeFromLastMsg}}</b> póŸniej{{else}}{{$time}}{{/if}}</span>
    {{if title}}<span class="floating title">{{$title}}</span>{{/if}}
    <div class="body">{{$body}}</div>
  </div>
</div>
{{else}}
<div class="row {{if sent?}}outgoing{{else}}incoming{{/if}}">
  <div class="context">
    <span class="closer" onclick="$(this.parentNode.parentNode).hideThread('slow');">x</span>
    <div class="header">
      <span class="sender">{{$htmlEscape(display)}}</span>
      <span class="time">{{$time}}</span>
      {{if timeFromLastMsg}}<span class="time"><b>{{$timeFromLastMsg}}</b> póŸniej</span>{{/if}}
      {{if title}}<span class="title">{{$title}}</span>{{/if}}
      {{if extInfo}}<span class="ext">{{$extInfo}}</span>{{/if}}
    </div>
    <div class="body">{{$body}}</div>
  </div>
</div>
{{/if}}