<div class="file">
  Plik
  {{if name}}
    <span class="filename">"<b>{{$name}}</b>"</span>
  {{/if}}
  {{if sent?}}
    wys�any do
  {{else}}
    odebrany od
  {{/if}}
  <span class="display"><b>{{$display}}</b></span> o
  <span class="time">{{func formatTime("%H:%M (%A)", @time)}}</span>:
  <br/><br/>

  {{if filePath}}
    <span class="path">�cie�ka: 
      {{if path}}
        <a href="file:///{{$path}}" target="_blank">{{$path}}</a> -
      {{/if}}
      <b><a href="file:///{{$filePath}}" target="_blank">{{$fileName}}</a></b>
    </span><br/>
  {{/if}}
  {{if size}}
    <span class="size">Rozmiar: <b>{{$size}}</b></span><br/>
  {{/if}}
  {{if transfered}}
    <span class="transfered">
      {{if isSent}}
        Przes�ano:
      {{else}}
        Odebrano:
      {{/if}}
      <b>{{$transferedP}}</b> ({{$transfered}} / {{$size}})
    </span><br/>
  {{/if}}
  {{if transferTime}}
    <span class="transfer-time">Czas: <b>{{$transferTime}}</b></span><br/>
  {{/if}}
  {{if avgSpeed}}
    <span class="transfer-speed">�rednia pr�dko��: <b>{{$avgSpeed}}</b></span><br/>
  {{/if}}
  {{if error}}
    <br/>
    <div class="transfer-error">B��d: <pre>{{$error}}</pre></div>
  {{/if}}

  <br/>
  <div class="body">{{$body}}</div>
</div>