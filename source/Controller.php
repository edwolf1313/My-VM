<?php

include "source/View.php";

class Controller{
	protected $view;
	public function __construct(){
		$this->view = new View();
	}
}

?>
