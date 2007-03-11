{if grouped?}
<dl>
  <dt><em>&nbsp;{unless groupTime?}{$formatTime('%H:%M', @time)}{else}{$time}{/unless}</em></dt>
  <dd>{$body}</dd>
</dl>
{else}
<div class="notGrouped {if sent?}out{else}in{/if}">
  <h5>{$getPluginName(@net)}</h5>
  <h2>{$htmlEscape(display)} <em>{$formatTime('%d/%m/%Y', @time)}</em></h2>
  <dl>
    <dt><em>{if inHistory?}{$formatTime('%H:%M', @time)}{else}{$time}{/if}</em></dt>
    <dd>{$body}</dd>
  </dl>
</div>
{/if}