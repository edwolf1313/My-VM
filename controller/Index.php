<?php
include "source/Controller.php";
class Index extends Controller {
	public function __construct(){
		parent::__construct();

	}
	public function index(){
		$this-> view -> set_view("view/home.php");
		$this-> view -> home();
	}
  
}

 ?>
