function cadastroSubmit() {
  const nome = document.getElementById('username').value
  const email = document.getElementById('email').value
  const senha = document.getElementById('password').value
  const confirmacao = document.getElementById('confirmpassword').value

  if (senha !== confirmacao) {
    alert('Senhas não conferem')

    return;
  }


  fetch('http://localhost:8888/api/register?name=' + nome + '&email=' + email + '&password=' + senha, {
    method: 'POST',
  }).then((response) => {
    if (response.status === 204) {
      alert('Cadastro efetuado com sucesso')
      window.location.href = 'login.html'
    } else {
      alert('Erro ao cadastrar')
    }
  })
}