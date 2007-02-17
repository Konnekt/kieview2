<div class="message {{if grouped}}messageGrouped{{else}}{{if sent?}}messageSent{{else}}messageReceived{{/if}}{{/if}}">
  <div class="header">
    {{unless grouped}}<span class="nick">{{$htmlEscape(display)}}:</span>{{/unless}}
    <span class="timestamp">{{if groupTime}}<b>{{$timeFromLastMsg}}</b> póŸniej{{else}}({{$time}}){{/if}}</span>
    {{if extInfo}}<span class="extinfo">{{$extInfo}}</span>{{/if}}
  </div>
  <div class="body">{{$body}}</div>
</div>