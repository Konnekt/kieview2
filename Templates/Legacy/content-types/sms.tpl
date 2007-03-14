<div class="sms">
  <div class="header">SMS
    {if from}od <span class="nick">{$htmlEscape(from)}</span>{/if}
    do <span class="dest">{$htmlEscape(to)}</span>
    wys³ano: <span class="timestamp">{$time}</span>
    bramka: <span class="extinfo">{$gate}</span>
  </div>
  <div class="body">
    {$body}
  </div>
</div>