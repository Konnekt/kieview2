<div class="sms">
  <div class="header">
    SMS {{if from}}od <span class="nick">{{$htmlEscape(from)}}</span>{{/if}}
    do <span class="dest">{{$htmlEscape(to)}}</span>
    wys³ano: <span class="timestamp">{{func formatTime("%A, %d.%m.%Y %H:%M", @time)}}</span>
    bramka: <span class="gate">{{$gate}}</span>
  </div>
  <div class="body">{{$body}}</div>
</div>