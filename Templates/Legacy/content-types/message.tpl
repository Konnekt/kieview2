<div class="{if grouped?}{if sent?}sendnext{else}recvnext{/if}{else}{if sent?}send{else}recv{/if}{/if}">
  <div class="header">
    {unless grouped?}<span class="nick">{$display}:</span>{/unless}
    {if time}<span class="timestamp">({$time})</span>{/if}
    {if extInfo}<span class="extinfo">{$extinfo}</span>{/if}
  </div>
  <div class="body">
    {$body}
  </div>
</div>