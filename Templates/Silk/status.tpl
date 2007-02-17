<div class="status">
  [<span class="timestamp">{{$time}}</span>]
  <span class="body">
    status <span class="name">{{$status}}</span>
    {{if ignored?}}- ignoruje Ciê{{/if}}
    {{if info}}<span class="info">"{{$info}}"</span>{{/if}}
  </span>
</div>