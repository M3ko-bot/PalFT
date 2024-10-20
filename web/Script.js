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
  const previous = localStorage.getItem('/state/item')

  if (previous) setItemData(previous, true)
  setItemData(number)

  localStorage.setItem('/state/item', number)
}

function setItemData(num, reverse) {
  document.getElementById(`ch${num}`).style.backgroundColor = reverse ? 'white' : 'blue'
  document.getElementById(`prod${num}`).style.display = reverse ? 'none' : 'block'
}