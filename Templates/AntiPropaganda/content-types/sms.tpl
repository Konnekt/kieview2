{{if grouped}}
<div class="row sms" style="margin-top: -10px">
  <div class="context-grouped">
    <span class="floating time">{{if groupTime}}<b>{{$timeFromLastSt}}</b> póŸniej{{else}}{{$time}}{{/if}}</span>
    <div class="body">{{$body}}</div>
  </div>
</div>
{{else}}
<div class="row sms">
  <div class="context">
    <div class="header">
      {{if from}}<span class="nick">{{$htmlEscape(from)}}</span>{{/if}}
      <span class="to">{{$htmlEscape(to)}}</span>
      <span class="time">{{func formatTime("%A, %d.%m.%Y %H:%M", @time)}}</span>
      <span class="gate">{{$gate}}</span>
    </div>
    <div class="body">{{$body}}</div>
  </div>
</div>
{{/if}}