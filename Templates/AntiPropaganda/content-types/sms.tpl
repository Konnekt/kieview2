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