<?php
header('Content-Type: application/json');
set_time_limit(0);
$function=$_POST['function_get'];
// $VIX_path='PATH=%PATH%;C:\Program Files (x86)\VMware\VMware VIX\Workstation-15.0.0\32bit';
// $Project_Path='cd /d E:\Semester 6\tekvir\work_tekvir\try_vm\Release';
$Project_Path='cd ..\VM';
$App_Name='try_vm_2.exe';

// $VMware_Path='D:\download\VMware\machine_vmware_2\CentOS version 5 and earlier 64-bit.vmx';
switch ($function) {
  case "on":
    turn_on();
  break;
  case "off":
    turn_off();
  break;
  case "suspend":
    suspend();
  break;
  case "script":
    script();
  break;
  case "fclone":
    full_clone();
  break;
  case "lclone":
    linked_clone();
  break;
  case "run_program":
    run_program();
  break;
  case "create_snapshot":
    create_snapshot();
  break;
  case "revert_snapshot":
    revert_snapshot();
  break;
  default:
    break;
}

function turn_on()
{
    global $Project_Path,$App_Name;
    $Command="START";
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command);
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function turn_off()
{
    global $Project_Path,$App_Name;
    $Command="STOP";
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command);
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function suspend()
{
    global $Project_Path,$App_Name;
    $Command="SUSPEND";
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command);
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function full_clone()
{
    global $Project_Path,$App_Name;
    $Command="FCLONE";
    $Clone_name = $_POST['name'];
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$Clone_name.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function linked_clone()
{
    global $Project_Path,$App_Name;
    $Command="LCLONE";
    $Clone_name = $_POST['name'];
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$Clone_name.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function script()
{
    global $Project_Path,$App_Name;
    define('UPLOAD_DIR', '..\public\script_uploaded\\');
    $script = $_POST['script'];
    $script = str_replace(array('data:text/plain;base64,','data:text/x-sh;base64,','data:application/octet-stream;base64,'), array('','',''), $script);
    $data = base64_decode($script);
    $file = UPLOAD_DIR . $_POST['file_name'];
    $success = file_put_contents($file, $data);
    //send request to ocr
    $Command="SCRIPT";
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$file.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function run_program()
{
    global $Project_Path,$App_Name;
    $Command="RUNPROGRAM";
    $command= $_POST['run_command'];
    $args= $_POST['run_args'];
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$command.'" "'.$args.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function create_snapshot()
{
    global $Project_Path,$App_Name;
    $Command="SNAPSHOT";
    $snapshot_name = $_POST['name'];
    $snapshot_description = $_POST['description'];
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$snapshot_name.'" "'.$snapshot_description.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}

function revert_snapshot()
{
    global $Project_Path,$App_Name;
    $Command="REVERTSNAPSHOT";
    $snapshot_name = $_POST['name'];
    $result=shell_exec($Project_Path.' & '.$App_Name.' '.$Command.' "'.$snapshot_name.'"');
    $respond=array("data"=> $result);
    echo json_encode($respond);
}
