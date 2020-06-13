<?php
class View
{
    protected $view="";
    protected $template= "";
    protected $assets_url="";

    public function __construct()
    {
        $root = str_replace("/", "\\", $_SERVER["DOCUMENT_ROOT"]);
        $path = realpath(__DIR__."/../assets");
        $this-> assets_url = str_replace($root, "http://36.82.141.83", $path);
        // echo realpath(__dir__)."<br>";
        // echo $path."<br>";
        // echo $root;
        $this-> assets_url = str_replace("\\", "/", $this->assets_url);
        $this-> template = "template/layout.php";
    }

    public function set_view($view)
    {
        $this->view = $view;
    }

    public function home()
    {
        $current_view = $this->view;
        $assets_url= $this->assets_url;
        include $this->template;
    }
}
