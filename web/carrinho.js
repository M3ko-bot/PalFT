//
function setCookie(name,value,days) {
  var expires = "";
  if (days) {
      var date = new Date();
      date.setTime(date.getTime() + (days*24*60*60*1000));
      expires = "; expires=" + date.toUTCString();
  }
  document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}
function getCookie(name) {
  var nameEQ = name + "=";
  var ca = document.cookie.split(';');
  for(var i=0;i < ca.length;i++) {
      var c = ca[i];
      while (c.charAt(0)==' ') c = c.substring(1,c.length);
      if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
  }
  return null;
}
function eraseCookie(name) {   
  document.cookie = name +'=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';
}
//

function fetchProdutos() {
  const token = getCookie('token')
  if (!token) {
    alert('Você precisa estar logado para adicionar produtos ao carrinho')

    return;
  }

  fetch(`http://localhost:8888/api/carrinho/lista?session=${token}`).then((response) => response.json()).then((data) => {
    /* INFO: Cria uma lista de produtos */
    const ul = document.createElement('ul')
    document.body.appendChild(ul)

    const tipos = {}
    data.forEach((produto) => {
      /* INFO: Cria um item de lista para cada produto */
      if (!tipos[produto.tipo]) tipos[produto.tipo] = []

      tipos[produto.tipo].push(produto)
    })

    const product_area = document.getElementById('product_area')
    Object.keys(tipos).forEach((tipo) => {
      const prods = document.createElement('div')
      prods.classList.add('prods')

      product_area.appendChild(prods)

      tipos[tipo].forEach((produto) => {
        const content = document.createElement('div')
        content.classList.add('content')

        prods.appendChild(content)

        const img = document.createElement('img')
        img.src = produto.imagem
        img.alt = ''
        img.classList.add('imagem_bixo')
        img.classList.add(produto.tipo)

        content.appendChild(img)
      })
    })
  })
}

fetchProdutos()