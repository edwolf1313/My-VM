<!DOCTYPE html>
<html lang="en" dir="ltr">

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <meta name="description" content="">
  <meta name="author" content="">
  <title>My-VM</title>
  <link rel="icon" href="<?php echo $assets_url; ?> /img/moon_icon.png">
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css">
  <link href="<?php echo $assets_url; ?> /vendor/fontawesome-free/css/all.min.css" rel="stylesheet" type="text/css">
  <!-- custom font  -->
  <link href="https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,700,300italic,400italic,700italic" rel="stylesheet" type="text/css">
  <link href="<?php echo $assets_url; ?> /vendor/simple-line-icons/css/simple-line-icons.css" rel="stylesheet">

  <!-- custom CSS -->
  <link href="<?php echo $assets_url; ?> /css/stylish-portfolio.css" rel="stylesheet">
</head>

<body id="page-top">
  <div class="page-loader" style="">
    <div class="loader">
      <span class="dot dot_1"></span>
      <span class="dot dot_2"></span>
      <span class="dot dot_3"></span>
      <span class="dot dot_4"></span>
    </div>
  </div>
  <?php include $current_view; ?>
  <!-- Footer -->
  <footer class="footer text-center">
    <div class="container">
      <ul class="list-inline mb-5">
        <li class="list-inline-item">
          <a class="social-link rounded-circle text-white" href="#">
            <i class="icon-social-github"></i>
          </a>
        </li>
      </ul>
      <p class="text-muted small mb-0">Copyright &copy; My-VM 2020</p>
    </div>
  </footer>
  <!-- Scroll to Top Button-->
  <a class="scroll-to-top rounded js-scroll-trigger" href="#page-top">
    <i class="fas fa-angle-up"></i>
  </a>
  <script src="<?php echo $assets_url; ?> /vendor/jquery/jquery.min.js"></script>
  <script src="<?php echo $assets_url; ?> /vendor/bootstrap/js/bootstrap.bundle.min.js"></script>
  <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/js/bootstrap.min.js" charset="utf-8"></script>
  <!-- Plugin JavaScript -->
  <script src="<?php echo $assets_url; ?> /vendor/jquery-easing/jquery.easing.min.js"></script>

  <!-- Custom scripts for this template -->
  <script src="<?php echo $assets_url; ?> /js/stylish-portfolio.min.js"></script>
  <script>
    function getBase64(file) {
      return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.readAsDataURL(file);
        reader.onload = () => resolve(reader.result);
        reader.onerror = error => reject(error);
      });
    }
    //passing command to modal;
    $(document).on("click", ".open-modalDialog", function() {
      var command = $(this).attr('name');
      $(".cloneModal").attr('name', command);
    });
    //file modal;
    $(document).on("click", ".open-scriptDialog", function() {
      var command = $(this).attr('name');
      $(".scriptModal").attr('name', command);
    });
    //file ;
    $(document).on('change', '#file_script', function() {
      var property = $(this).prop('files')[0];
      var script_name = property.name;
      var script_extension = script_name.split('.').pop().toLowerCase();
      if (jQuery.inArray(script_extension, ['sh', 'pl', 'py', 'php']) == -1) {
        alert("Invalid script type only .sh, .py, .php, .pl");
        $(this).val(null);
      }
    })

    function ajax_function($command, $name, $file_name, $scriptload) {
      $(".page-loader").show();
      $.ajax({
        'url': '/My-VM/services/command_functions.php',
        'method': 'POST',
        'data': {
          'function_get': $command,
          'clone_name': $name,
          'file_name': $file_name,
          'script': $scriptload,
        },
        'cache': false,
        beforeSend: function() {},
        'success': function(data) {
          $(".page-loader").hide();
          alert(data["data"]);
          //  data.forEach(function(item,index){
          // });

        },
        'error': function(data) {
          $(".page-loader").hide();
          console.log(data);
        }
      });
    }
    $('.command').on('click', function() {
      $command = $(this).attr("name");
      $scriptload = "nothing";
      $file_name = "My-VMfile";
      $name = "nn-VM";
      if ($command != "script") {
        $name = $("#clone_name").val();
        ajax_function($command, $name, "", "");
      } else if ($command == "script") {
        $property = $("#file_script")[0].files[0];
        getBase64($property).then(function(value) {
          ajax_function($command, $name, $property.name, value);
        });
      }
    });
    $(document).ready(function() {
      $(".page-loader").hide();
    })
  </script>
</body>

</html>
