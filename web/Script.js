let slideIndex = 0
showSlides();

function showSlides() {
    let i;
    let slides = document.getElementsByClassName("slide");
    for (i = 0; i < slides.length; i++) {
        slides[i].style.display = "none";
    }
    slideIndex++;
    if (slideIndex > slides.length) { slideIndex = 1 }
    slides[slideIndex - 1].style.display = "block";
    setTimeout(showSlides, 2000);
}


function show1(){
    document.getElementById('ch1').style.backgroundColor ='blue';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='block';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='none';
    console.log("showing dragons")
  }
function show2(){
  document.getElementById('ch1').style.backgroundColor ='white';
  document.getElementById('ch2').style.backgroundColor ='blue';
  document.getElementById('ch3').style.backgroundColor ='white';
  document.getElementById('ch4').style.backgroundColor ='white';
  document.getElementById('ch5').style.backgroundColor ='white';
  document.getElementById('ch6').style.backgroundColor ='white';
  document.getElementById('ch7').style.backgroundColor ='white';
  document.getElementById('ch8').style.backgroundColor ='white';
  document.getElementById('ch9').style.backgroundColor ='white';
  document.getElementById('prod1').style.display ='none';
  document.getElementById('prod2').style.display ='block';
  document.getElementById('prod3').style.display ='none';
  document.getElementById('prod4').style.display ='none';
  document.getElementById('prod5').style.display ='none';
  document.getElementById('prod6').style.display ='none';
  document.getElementById('prod7').style.display ='none';
  document.getElementById('prod8').style.display ='none';
  document.getElementById('prod9').style.display ='none';
    console.log("showing fire")
  }

  function show3(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='blue';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='block';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='none';
    console.log("showing grass")
  }
function show4(){
  document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
        document.getElementById('ch3').style.backgroundColor ='white';
              document.getElementById('ch4').style.backgroundColor ='blue';
                        document.getElementById('ch5').style.backgroundColor ='white';
                                        document.getElementById('ch6').style.backgroundColor ='white';
                                                                  document.getElementById('ch7').style.backgroundColor ='white';
                                                                                                            document.getElementById('ch8').style.backgroundColor ='white';
                                                                                                                                                                                document.getElementById('ch9').style.backgroundColor ='white';
                                                                                                                                                                                document.getElementById('prod1').style.display ='none';
                                                                                                            document.getElementById('prod2').style.display ='none';
                                                                  document.getElementById('prod3').style.display ='none';
                                        document.getElementById('prod4').style.display ='block';
                        document.getElementById('prod5').style.display ='none';
              document.getElementById('prod6').style.display ='none';
        document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
  document.getElementById('prod9').style.display ='none';
  console.log("showing neutral")
  }
  function show5(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='blue';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='block';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='none';
    console.log("showing eletric")
  }

  function show6(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='blue';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='block';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='none';
    console.log("showing ground")
  }

  function show7(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='blue';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='block';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='none';
    console.log("showing ice")
  }

  function show8(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='blue';
    document.getElementById('ch9').style.backgroundColor ='white';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='block';
    document.getElementById('prod9').style.display ='none';
    console.log("showing dark")
  }
  
  function show9(){
    document.getElementById('ch1').style.backgroundColor ='white';
    document.getElementById('ch2').style.backgroundColor ='white';
    document.getElementById('ch3').style.backgroundColor ='white';
    document.getElementById('ch4').style.backgroundColor ='white';
    document.getElementById('ch5').style.backgroundColor ='white';
    document.getElementById('ch6').style.backgroundColor ='white';
    document.getElementById('ch7').style.backgroundColor ='white';
    document.getElementById('ch8').style.backgroundColor ='white';
    document.getElementById('ch9').style.backgroundColor ='blue';
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
    document.getElementById('prod5').style.display ='none';
    document.getElementById('prod6').style.display ='none';
    document.getElementById('prod7').style.display ='none';
    document.getElementById('prod8').style.display ='none';
    document.getElementById('prod9').style.display ='block';
    console.log("showing water")
  }