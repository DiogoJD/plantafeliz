<?php



$temperatura = json_decode(file_get_contents("http://arduinouno/temperature"), true);
$humidade    = json_decode(file_get_contents("http://arduinouno/humidity"), true);

echo "Humidade: ".$humidade['humidity']."<br>Temperatura: ".$temperatura['temperature']


?>