let slideIndex = 0
showSlides();

// INFO: Currently not usable
function showSlides() {
  let i
  let slides = document.getElementsByClassName("slide")

  for (i = 0; i < slides.length; i++) {
    slides[i].style.display = "none"
  }

  slideIndex++
  if (slideIndex > slides.length) { slideIndex = 1 }

  slides[slideIndex - 1].style.display = "block"

  setTimeout(showSlides, 2000)
}

function showItem(number) {
  const chs = document.getElementsByClassName('chs')
  const prods = document.getElementsByClassName('prods')

  for (let i = 0; i < chs.length; i++) {
    chs[i].style.backgroundColor = 'white'
    prods[i].style.display = 'none'
  }

  chs[number - 1].style.backgroundColor = 'blue'
  prods[number - 1].style.display = 'block'
}
