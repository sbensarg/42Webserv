<?php
session_start(); 

$cookie_name = "user";
$cookie_value = "zoizmer milmi";
setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/");
printf("<h1>Upload your file</h1>");
?>
<!DOCTYPE html>
<html>
<head>
  <title>PHP File Upload</title>
</head>
<body>
  <?php
  	$cookie_name = "user";
    if (isset($_SESSION['message']) && $_SESSION['message'])
    {
      printf('<b>%s</b>', $_SESSION['message']);
      unset($_SESSION['message']);
      printf('<b>%s</b>', $_COOKIE[$cookie_name]);

    }
  ?>
  <form method="POST" action="upload.php" enctype="multipart/form-data">
    <div>
      <span>Upload a File:</span>
      <input type="file" name="uploadedFile" />
    </div>

    <input type="submit" name="uploadBtn" value="Upload" />
  </form>
</body>
</html>