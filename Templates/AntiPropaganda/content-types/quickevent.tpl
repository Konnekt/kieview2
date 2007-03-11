{if grouped?}
<div class="row grouped quickevent" style="margin-top: -10px">
  <div class="context-grouped">
    {if showTime?}<span class="floating time">{$time}</span>{/if}
    <div class="body">{$body}</div>
  </div>
</div>
{else}
<div class="row quickevent">
  <div class="context">
    <span class="closer" onclick="$(this.parentNode.parentNode).hideThread('slow');">x</span>
    <div class="header">
      {if showTime?}<span class="time">{$time}</span>{/if}
      <span class="type">{if warning}warning{else}info{/if}</span>
    {unless match?('|<br ?/?>|', body)}
      <span class="body">{$body}</span>
    </div>
      {else}
    </div>
    <div class="body">{$body}</div>
    {/unless}
  </div>
</div>
{/if}