<div class="notGrouped {if sent?}out{else}in{/if}">
  <h2>{$name} <em>{$time}</em></h2>
  <dl>
    <dd>
    {if path}
      <div class="path">
        <span class="value">
          <a href="file:///{$path}" target="_blank">{$path}</a> -
          <b><a href="file:///{$filePath}" target="_blank">{$fileName}</a></b>
        </span>
      </div>
      <br/>
    {/if}
    {if size}
      <div class="filesize">Rozmiar: <span class="value">{$size}</span></div>
    {/if}
    {if transfered}
      <div class="filetransfered">
        {if sent?}Przes³ano{else}Odebrano{/if}:
        <span class="value"><b>{$transferedP}</b> ({$transfered} / {$size})</span>
      </div>
    {/if}
    {if transferTime}
      <div class="filetransfertime">Czas: <span class="value">{$transferTime}</span></div>
    {/if}
    {if avgSpeed}
      <div class="filetransfer">Œrednia prêdkoœæ: <span class="value">{$avgSpeed}</span></div>
    {/if}
    {if error}
      <div class="fileerror">B³¹d: <span class="value">{$error}</span></div>
    {/if}
    {if body}
      <div class="rest">{$body}</div>
    {/if}
    </dd>
  </dl>
</div>