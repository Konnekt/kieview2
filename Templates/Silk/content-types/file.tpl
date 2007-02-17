<div class="file {{if sent?}}fileSent{{else}}fileReceived{{/if}}">
  <div class="header">
    Plik {{if name}}<span class="filename">"{{$name}}"</span>{{/if}}
    {{if sent?}}wys³any do{{else}}odebrany od{{/if}}
    <span class="nick">{{$display}}</span>
    <span class="timestamp">{{func formatTime("(%A, %d.%m.%Y %H:%M)", @time)}}</span>
  </div>
  <div class="rest">
  {{if path}}
    <div class="path">Œcie¿ka:
      <span class="value">
        <a href="file:///{{$path}}" target="_blank">{{$path}}</a> -
        <b><a href="file:///{{$filePath}}" target="_blank">{{$fileName}}</a></b>
      </span>
    </div>
  {{/if}}
  {{if size}}
    <div class="filesize">Rozmiar: <span class="value">{{$size}}</span></div>
  {{/if}}
  {{if transfered}}
    <div class="filetransfered">
      {{if sent?}}Przes³ano{{else}}Odebrano{{/if}}:
      <span class="value"><b>{{$transferedP}}</b> ({{$transfered}} / {{$size}})</span>
    </div>
  {{/if}}
  {{if transferTime}}
    <div class="filetransfertime">Czas: <span class="value">{{$transferTime}}</span></div>
  {{/if}}
  {{if avgSpeed}}
    <div class="filetransfer">Œrednia prêdkoœæ: <span class="value">{{$avgSpeed}}</span></div>
  {{/if}}
  {{if error}}
    <div class="fileerror">B³¹d: <span class="value">{{$error}}</span></div>
  {{/if}}
  {{if body}}
    <div class="body">{{$body}}</div>
  {{/if}}
  </div>
</div>