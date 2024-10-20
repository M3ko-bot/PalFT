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
        img.src = `images/${produto.tipo}/${produto.id}.png`
        img.alt = ''
        img.classList.add('imagem_bixo')
        img.classList.add(produto.tipo)

        content.appendChild(img)

        const button = document.createElement('button')
        button.onclick = () => {
          /* TODO: Adicionar produto ao carrinho */
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