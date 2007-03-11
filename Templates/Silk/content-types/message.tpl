<div class="message {if grouped?}messageGrouped{else}{if sent?}messageSent{else}messageReceived{/if}{/if}">
  <div class="header">
    {unless grouped?}<span class="nick">{$htmlEscape(display)}:</span>{/unless}
    {if time}<span class="timestamp">({$time})</span>{/if}
    {if title}<span class="extinfo">{$title}</span>{/if}
  </div>
  <div class="body">{$body}</div>
</div>