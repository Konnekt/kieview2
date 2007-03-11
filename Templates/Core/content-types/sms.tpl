<div class="message sms">
  {if from}
    <span class="from">od <b>{$htmlEscape(from)}</b></span>
  {/if}
  <span class="to">do <b>{$htmlEscape(to)}</b></span>
  <span class="time">({$time})</span>,
  <span class="gate">bramka: <b>{$gate}</b></span>
  <div class="body">{$body}</div>
</div>