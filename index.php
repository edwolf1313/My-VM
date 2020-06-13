<?php
$url = array();
$controllerName = "";
$functionName = "";
// if(isset($_GET['path'])){
// 	$url = explode("/",$_GET['path']);
// 	$controllerName = $url[0];
//
// 	if(count($url)>1 && $url[1]!=""){
// 		$functionName = $url[1];
// 	}
// 	else{
// 		$functionName = "index";
// 	}
// }
// else{
// 	$controllerName = "Index";
// 	$functionName = "index";
// }
$controllerName = "Index";
$functionName = "index";
require_once("controller/" .$controllerName. ".php");

$controller = new $controllerName(); //new Index();
$controller -> $functionName(); // index
//power on
//power off
//suspend
//execution program
