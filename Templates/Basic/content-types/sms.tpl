{if grouped?}
<dl>
  <dt><em>&nbsp;{unless groupTime?}{$formatTime('%H:%M', @time)}{else}{$time}{/unless}</em></dt>
  <dd>{$body}</dd>
</dl>
{else}
<div class="sms notGrouped out">
  <h2>{$htmlEscape(to)} <em>{$formatTime('%d/%m/%Y', @time)}</em></h2>
  <dl>
    <dt><em>{$formatTime('%H:%M', @time)}</em></dt>
    <dd>{$body}</dd>
  </dl>
</div>
<div class="smsFooter">
  via <strong>{$gate}</strong> {if from}, by <strong>{$htmlEscape(from)}</strong>{/if}
</div>
{/if}