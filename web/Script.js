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

function showItem(number) {
  const dataTag = document.getElementById('cache-click-state')
  const current = dataTag.getAttribute('content')

  if (current?.length !== 0) {
    document.getElementById(`ch${current}`).style.backgroundColor = 'white'
    document.getElementById(`prod${current}`).style.display = 'none'
  }

  document.getElementById(`ch${number}`).style.backgroundColor = 'blue'
  document.getElementById(`prod${number}`).style.display = 'block'

  dataTag.setAttribute('content', number)
}

function addCarrinho(produto) {
  const loginToken = getCookie('token')
  if (!loginToken) {
    alert('Você precisa estar logado para adicionar produtos ao carrinho')
    return;
  }

  fetch('http://localhost:8888/api/carrinho/add?produto=' + produto + '&session=' + loginToken, {
    method: 'POST',
  }).then((response) => {
    if (response.status === 204) {
      alert('Produto adicionado ao carrinho')
    } else {
      alert('Erro ao adicionar produto ao carrinho')
    }
  })
}


function fetchProdutos() {
  fetch('http://localhost:8888/api/produtos/lista').then((response) => response.json()).then((data) => {
    /* INFO: Cria uma lista de produtos */
    const ul = document.createElement('ul');
    document.body.appendChild(ul);

    const tipos = {}
    data.forEach((produto) => {
      /* INFO: Cria um item de lista para cada produto */
      if (!tipos[produto.tipo]) tipos[produto.tipo] = []

      tipos[produto.tipo].push(produto)
    })

    const filter = document.getElementById('filter')
    Object.keys(tipos).forEach((tipo, index) => {
      /*
        <label for="ch1" class="tipo_palft">
          <input type="radio" name="tab" id="ch1" class="chs" onclick="showItem(1)">
          Dragon
        </label>
      */
      const ch = document.createElement('label')
      ch.htmlFor = `ch${index}`
      ch.classList.add('tipo_palft')

      filter.appendChild(ch)

      const input = document.createElement('input')
      input.type = 'radio'
      input.name = 'tab'
      input.id = `ch${index}`
      input.classList.add('chs')
      input.onclick = () => showItem(index + 1)

      ch.appendChild(input)

      const tipoCapitalized = tipo.charAt(0).toUpperCase() + tipo.slice(1)

      const text = document.createTextNode(tipoCapitalized)
      ch.appendChild(text)
    })

    const product_area = document.getElementById('product_area')
    Object.keys(tipos).forEach((tipo) => {
      /*                 
        <div class="prods">
          <div class="content">
            <img src="images/fire/71.png" alt="" class="palft" class="fire">
            <button onclick="" class="pal_button">Comprar</button>
          </div>
          <div class="content">
            <img src="images/fire/58.png" alt="" class="palft" class="fire">
            <button onclick="" class="pal_button">Comprar</button>
          </div>
        </div> 
      */
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

        const button = document.createElement('button')
        button.onclick = () => {
          addCarrinho(produto.nome)
        }
        button.classList.add('pal_button')

        const text = document.createTextNode('Comprar')
        button.appendChild(text)

        content.appendChild(button)
      })
    })
  })
}

fetchProdutos()