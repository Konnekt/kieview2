<div class="message quickevent">
  {if showTime?}
    <span class="time">[<b>{$time}</b>]</span>
  {/if}
  <div class="body">
    {if warning?}! <b>warning</b> !{/if}
    {$body}
  </div>
</div>