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
    document.getElementById('prod1').style.display ='block';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
  }
function show2(){
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='block';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='none';
  }

  function show3(){
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='block';
    document.getElementById('prod4').style.display ='none';
  }
function show4(){
    document.getElementById('prod1').style.display ='none';
    document.getElementById('prod2').style.display ='none';
    document.getElementById('prod3').style.display ='none';
    document.getElementById('prod4').style.display ='block';
  }