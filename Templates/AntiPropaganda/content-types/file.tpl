<div class="row file">
  <div class="context">
    <div class="header">
      <span class="name">Plik <b>{{$name}}</b> {{if sent?}}wys³any do{{else}}odebrany od{{/if}} <b>{{$display}}</b></span>
      <span class="time">{{func formatTime("%A, %d.%m.%Y %H:%M", @time)}}</span>
    </div>
    <!-- @todo -->
    <div class="body">
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
      <div class="rest">{{$body}}</div>
    {{/if}}
    </div>
  </div>
</div>