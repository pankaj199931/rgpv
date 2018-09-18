<?php
$username= $_POST['username'];
$branch  = $_POST['branch'];
$email  = $_POST['email'];
$phone  = $_POST['phone'];
$year  = $_POST['Year'];
if (!empty($username) || !empty($branch) || !empty($email) || !empty($phone) ||
!empty($year) ) {
 $host = "localhost";
    $dbUsername = "root";
    $dbPassword = "";
    $dbname = "mydb";
    //create connection
    $conn = new mysqli($host, $dbUsername, $dbPassword, $dbname);
    if (mysqli_connect_error()) {
     die('Connect Error('. mysqli_connect_errno().')'. mysqli_connect_error());
    } else {
     $SELECT = "SELECT email From register Where email = ? Limit 1";
     $INSERT = "INSERT Into register (username,branch,email,phone,year) values(?, ?, ?, ?, ?)";
     //Prepare statement
     $stmt = $conn->prepare($SELECT);
     $stmt->bind_param("s", $email);
     $stmt->execute();
     $stmt->bind_result($email);
     $stmt->store_result();
     $rnum = $stmt->num_rows;
     if ($rnum==0) {
      $stmt->close();
      $stmt = $conn->prepare($INSERT);
      $stmt->bind_param("sssis", $username, $branch, $email, $phone, $year);
      $stmt->execute();
      header("Location: page2.html");
     } else {
      header("Location: page3.html");
     }
     $stmt->close();
     $conn->close();
    }
} else {
  header("Location: page4.html");
 die();
}
?>