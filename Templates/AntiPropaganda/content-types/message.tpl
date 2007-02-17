{{if grouped}}
<div class="row {{if sent?}}outgoing{{else}}incoming{{/if}}" style="margin-top: -10px">
  <div class="context-grouped">
    <span class="time">{{if groupTime}}<b>{{$timeFromLastMsg}}</b> póŸniej{{else}}{{$time}}{{/if}}</span>
    <div class="body">{{$body}}</div>
  </div>
</div>
{{else}}
<div class="row {{if sent?}}outgoing{{else}}incoming{{/if}}">
  <div class="context">
    <div class="header">
      <span class="sender">{{$htmlEscape(display)}}</span>
      <span class="time">{{$time}}</span>
      {{if extInfo}}<span class="ext">{{$extInfo}}</span>{{/if}}
    </div>
    <div class="body">{{$body}}</div>
  </div>
</div>
{{/if}}